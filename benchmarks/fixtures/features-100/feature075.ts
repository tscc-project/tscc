function id75<T>(x: T): T { return x; }
class Base75<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child75 extends Base75<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E75 { A, B = 3, C, Label = "label" }
const x75: number = id75<number>(75);
const c75 = new Child75("n", x75);
console.log(c75.name, E75.A, E75[3]);
