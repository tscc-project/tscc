function id4<T>(x: T): T { return x; }
class Base4<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child4 extends Base4<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E4 { A, B = 3, C, Label = "label" }
const x4: number = id4<number>(4);
const c4 = new Child4("n", x4);
console.log(c4.name, E4.A, E4[3]);
