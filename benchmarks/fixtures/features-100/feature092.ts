function id92<T>(x: T): T { return x; }
class Base92<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child92 extends Base92<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E92 { A, B = 3, C, Label = "label" }
const x92: number = id92<number>(92);
const c92 = new Child92("n", x92);
console.log(c92.name, E92.A, E92[3]);
