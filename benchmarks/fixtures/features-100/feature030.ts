function id30<T>(x: T): T { return x; }
class Base30<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child30 extends Base30<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E30 { A, B = 3, C, Label = "label" }
const x30: number = id30<number>(30);
const c30 = new Child30("n", x30);
console.log(c30.name, E30.A, E30[3]);
