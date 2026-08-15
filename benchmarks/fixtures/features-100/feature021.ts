function id21<T>(x: T): T { return x; }
class Base21<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child21 extends Base21<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E21 { A, B = 3, C, Label = "label" }
const x21: number = id21<number>(21);
const c21 = new Child21("n", x21);
console.log(c21.name, E21.A, E21[3]);
