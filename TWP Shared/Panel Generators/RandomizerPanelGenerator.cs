using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using TheWitnessPuzzles;

namespace TWP_Shared
{
    class RandomizerPanelGenerator : PanelGenerator
    {
        [DllImport("RandomizerLibrary.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr GenerateSamplePuzzle();
        [DllImport("RandomizerLibrary.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr GenerateRandomPuzzle(int seed);

        public static RandomizerPanelGenerator Instance = new RandomizerPanelGenerator();
        readonly static Random seedGenerator = new Random();

        private int GetRawXFromNodeID(int nodeID, int rawWidth, int rawHeight)
        {
            return (nodeID % (rawWidth / 2 + 1)) * 2;
        }
        private int GetRawYFromNodeID(int nodeID, int rawWidth, int rawHeight)
        {
            return (nodeID / (rawWidth / 2 + 1)) * 2;
        }

        private Microsoft.Xna.Framework.Color NameToColor(string name)
        {
            switch(name)
            {
                case "black":
                    return Microsoft.Xna.Framework.Color.Black;
                case "white":
                    return Microsoft.Xna.Framework.Color.White;
                case "blue":
                    return Microsoft.Xna.Framework.Color.Blue;
                case "red":
                    return Microsoft.Xna.Framework.Color.Red;
                case "yellow":
                    return Microsoft.Xna.Framework.Color.Yellow;
                case "green":
                    return Microsoft.Xna.Framework.Color.LightGreen;
                case "cyan":
                    return Microsoft.Xna.Framework.Color.Aqua;
                case "orange":
                    return Microsoft.Xna.Framework.Color.OrangeRed;
                case "magenta":
                    return Microsoft.Xna.Framework.Color.Magenta;
                case "purple":
                    return Microsoft.Xna.Framework.Color.Purple;
                default:
                    return Microsoft.Xna.Framework.Color.Pink;
            }
        }
        private Puzzle DecodePanelFromJson(string jsonString, ColorPalettesLibrary.ColorPalette panelPalette, int seed)
        {
            JObject json = JObject.Parse(jsonString);
            int rawWidth = json["width"].ToObject<int>();
            int rawHeight = json["height"].ToObject<int>();
            string symmetry = json["symmetry"].ToString();
            Puzzle puzzle;
            if (symmetry == "none")
            {
                puzzle = new Puzzle(rawWidth / 2, rawHeight / 2,
                    panelPalette.SingleLineColor, panelPalette.BackgroundColor,
                    panelPalette.WallsColor, panelPalette.ButtonsColor, seed);
            }
            else if (symmetry == "vertical" || symmetry == "rotational")
            {
                puzzle = new SymmetryPuzzle(rawWidth / 2, rawHeight / 2,
                    symmetry == "rotational", false, panelPalette.MainLineColor, panelPalette.MirrorLineColor,
                    panelPalette.BackgroundColor, panelPalette.WallsColor, panelPalette.ButtonsColor, seed);

            }
            else throw new Exception("Unsupported symmetry "+symmetry);
            JArray jsonGrid = json["grid"] as JArray;
            int p = 0;
            for (int x = 0; x < rawWidth; ++x)
            {
                for (int y = 0; y < rawHeight; ++y)
                {
                    
                }
            }
            for (int i = 0; i < puzzle.Nodes.Length; ++i)
            {
                int rawX = GetRawXFromNodeID(i, rawWidth, rawHeight);
                int rawY = GetRawYFromNodeID(i, rawWidth, rawHeight);
                int rawP = rawX * rawHeight + rawY;
                string type = jsonGrid[rawP]["type"].ToString();
                if (jsonGrid[rawP]["start"] != null)
                    puzzle.Nodes[i].AddState(NodeState.Start); // TODO: start and end on the same point?
                if (jsonGrid[rawP]["end"] != null)
                    puzzle.Nodes[i].AddState(NodeState.Exit); // TODO: start and mark on the same point?
                if (type == "dot")
                {
                    string color = jsonGrid[rawP]["color"].ToString();
                    if (color != "black")
                        puzzle.Nodes[i].AddStateAndColor(NodeState.Marked, color == "first" ? panelPalette.MainLineColor : panelPalette.MirrorLineColor);
                    else
                        puzzle.Nodes[i].AddState(NodeState.Marked);

                }
            }
            for(int j=0;j<puzzle.Edges.Count;++j)
            {
                Edge edge = puzzle.Edges[j];
                Node nodeA = puzzle.Edges[j].NodeA;
                Node nodeB = puzzle.Edges[j].NodeB;
                int rawX = (GetRawXFromNodeID(nodeA.Id, rawWidth, rawHeight) + GetRawXFromNodeID(nodeB.Id, rawWidth, rawHeight)) / 2;
                int rawY = (GetRawYFromNodeID(nodeA.Id, rawWidth, rawHeight) + GetRawYFromNodeID(nodeB.Id, rawWidth, rawHeight)) / 2;
                int rawP = rawX * rawHeight + rawY;
                string type = jsonGrid[rawP]["type"].ToString();
                if (type == "gap")
                    edge.SetState(EdgeState.Broken); // TODO: start and end?
                if (type == "dot")
                {
                    string color = jsonGrid[rawP]["color"].ToString();
                    if (color != "black")
                        edge.SetStateAndColor(EdgeState.Marked, color == "first" ? panelPalette.MainLineColor : panelPalette.MirrorLineColor);
                    else
                        edge.SetState(EdgeState.Marked);
                }
            }
            foreach(Block block in puzzle.Blocks)
            {
                int rawX = (GetRawXFromNodeID(block.Nodes[0].Id, rawWidth, rawHeight) + GetRawXFromNodeID(block.Nodes[1].Id, rawWidth, rawHeight) + 
                    GetRawXFromNodeID(block.Nodes[2].Id, rawWidth, rawHeight) + GetRawXFromNodeID(block.Nodes[3].Id, rawWidth, rawHeight)) / 4;
                int rawY = (GetRawYFromNodeID(block.Nodes[0].Id, rawWidth, rawHeight) + GetRawYFromNodeID(block.Nodes[1].Id, rawWidth, rawHeight) +
                    GetRawYFromNodeID(block.Nodes[2].Id, rawWidth, rawHeight) + GetRawYFromNodeID(block.Nodes[3].Id, rawWidth, rawHeight)) / 4;
                int rawP = rawX * rawHeight + rawY;
                string type = jsonGrid[rawP]["type"].ToString();
                string color = jsonGrid[rawP]["color"]?.ToString();
                if (type == "square")
                    block.Rule = new ColoredSquareRule(NameToColor(color));
                if (type == "star")
                    block.Rule = new SunPairRule(NameToColor(color));
                if (type == "eraser")
                    block.Rule = new EliminationRule(NameToColor(color));
                if (type=="poly" || type=="ylop")
                {
                    int intShape = jsonGrid[rawP]["shape"].ToObject<int>();
                    bool subtractive = type == "ylop";
                    int rotation = jsonGrid[rawP]["rotation"].ToObject<int>();
                    int maxX = 0, maxY = 0;
                    int minX = 4, minY = 4;
                    for (int x = 0; x < 4; ++x)
                    {
                        for (int y = 0; y < 4; ++y)
                        {
                            if ((intShape & (1 << (x + (3 - y) * 4))) > 0)
                            {
                                maxX = Math.Max(maxX, x);
                                maxY = Math.Max(maxY, y);
                                minX = Math.Min(minX, x);
                                minY = Math.Min(minY, y);
                            }
                        }
                    }
                    if(minX>maxX)
                    {
                        maxX = minX = maxY = minY = 0;
                    }
                    bool[,] binaryShape = new bool[maxX - minX + 1, maxY - minY + 1];
                    for (int x = minX; x <= maxX; ++x)
                    {
                        for (int y = minY; y <= maxY; ++y)
                        {
                            if ((intShape & (1 << (x + (3 - y) * 4))) > 0)
                                binaryShape[x - minX, y - minY] = true;
                        }
                    }
                    if (rotation == 0)
                        block.Rule = new TetrisRule(binaryShape, subtractive, NameToColor(color));
                    else
                        block.Rule = new TetrisRotatableRule(binaryShape, subtractive, NameToColor(color));
                }
                if (type == "triangle")
                    block.Rule = new TriangleRule(jsonGrid[rawP]["number"].ToObject<int>(), NameToColor(color));
            }
            return puzzle;
        }
        public override Puzzle GeneratePanel(int? seed = null)
        {
            if (seed == null)
                seed = seedGenerator.Next(1000000000);
            Random rnd = new Random(seed.Value);
            IntPtr ptr = GenerateRandomPuzzle(seed.Value);
            string jsonString = Marshal.PtrToStringAnsi(ptr);
            ColorPalettesLibrary.ColorPalette panelPalette = ColorPalettesLibrary.Palettes[rnd.Next(ColorPalettesLibrary.Size)];
            return DecodePanelFromJson(jsonString, panelPalette, seed.Value);
        }
    }
}
