function id1<T>(x: T): T { return x; }
class Base1<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child1 extends Base1<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E1 { A, B = 3, C, Label = "label" }
const x1: number = id1<number>(1);
const c1 = new Child1("n", x1);
console.log(c1.name, E1.A, E1[3]);
