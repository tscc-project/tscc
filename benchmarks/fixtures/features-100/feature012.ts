function id12<T>(x: T): T { return x; }
class Base12<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child12 extends Base12<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E12 { A, B = 3, C, Label = "label" }
const x12: number = id12<number>(12);
const c12 = new Child12("n", x12);
console.log(c12.name, E12.A, E12[3]);
