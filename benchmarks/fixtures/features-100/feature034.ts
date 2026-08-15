function id34<T>(x: T): T { return x; }
class Base34<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child34 extends Base34<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E34 { A, B = 3, C, Label = "label" }
const x34: number = id34<number>(34);
const c34 = new Child34("n", x34);
console.log(c34.name, E34.A, E34[3]);
