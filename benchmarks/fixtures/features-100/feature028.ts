function id28<T>(x: T): T { return x; }
class Base28<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child28 extends Base28<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E28 { A, B = 3, C, Label = "label" }
const x28: number = id28<number>(28);
const c28 = new Child28("n", x28);
console.log(c28.name, E28.A, E28[3]);
