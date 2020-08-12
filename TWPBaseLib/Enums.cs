using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TheWitnessPuzzles
{
    public enum EdgeState { Normal, Broken, Marked }
    [Flags]
    public enum NodeState { Normal = 0, Start = 1, Exit = 2, Marked = 4 }
    public enum TetrisShape { Dot, I_2, I_2_horiz, Square_2, I_3, I_3_horiz, S, Z, L, Г }
}
