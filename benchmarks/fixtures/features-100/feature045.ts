function id45<T>(x: T): T { return x; }
class Base45<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child45 extends Base45<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E45 { A, B = 3, C, Label = "label" }
const x45: number = id45<number>(45);
const c45 = new Child45("n", x45);
console.log(c45.name, E45.A, E45[3]);
