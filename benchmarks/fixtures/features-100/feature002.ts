function id2<T>(x: T): T { return x; }
class Base2<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child2 extends Base2<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E2 { A, B = 3, C, Label = "label" }
const x2: number = id2<number>(2);
const c2 = new Child2("n", x2);
console.log(c2.name, E2.A, E2[3]);
