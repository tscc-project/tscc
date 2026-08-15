function id35<T>(x: T): T { return x; }
class Base35<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child35 extends Base35<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E35 { A, B = 3, C, Label = "label" }
const x35: number = id35<number>(35);
const c35 = new Child35("n", x35);
console.log(c35.name, E35.A, E35[3]);
