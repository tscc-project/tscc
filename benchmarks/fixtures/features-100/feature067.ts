function id67<T>(x: T): T { return x; }
class Base67<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child67 extends Base67<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E67 { A, B = 3, C, Label = "label" }
const x67: number = id67<number>(67);
const c67 = new Child67("n", x67);
console.log(c67.name, E67.A, E67[3]);
