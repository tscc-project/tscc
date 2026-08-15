
function maybe<T>(x?: T): T | undefined { return x; }
class Fields {
  definite!: number;
  optional?: string;
  constructor(public readonly id: number, private label?: string) {
    this.definite = id;
  }
  getLabel(): string | undefined { return this.label; }
}
enum Numbers { Neg = -2, Next, Hex = 0x10, HexNext, Bin = 0b10, BinNext }
console.log(maybe<number>(3), new Fields(7, "x").getLabel(),
            Numbers.Neg, Numbers.Next, Numbers.HexNext, Numbers.BinNext);
