function id47<T>(x: T): T { return x; }
class Base47<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child47 extends Base47<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E47 { A, B = 3, C, Label = "label" }
const x47: number = id47<number>(47);
const c47 = new Child47("n", x47);
console.log(c47.name, E47.A, E47[3]);
