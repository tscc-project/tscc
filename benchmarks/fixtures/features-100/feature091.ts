function id91<T>(x: T): T { return x; }
class Base91<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child91 extends Base91<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E91 { A, B = 3, C, Label = "label" }
const x91: number = id91<number>(91);
const c91 = new Child91("n", x91);
console.log(c91.name, E91.A, E91[3]);
