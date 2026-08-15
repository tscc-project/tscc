function id74<T>(x: T): T { return x; }
class Base74<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child74 extends Base74<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E74 { A, B = 3, C, Label = "label" }
const x74: number = id74<number>(74);
const c74 = new Child74("n", x74);
console.log(c74.name, E74.A, E74[3]);
