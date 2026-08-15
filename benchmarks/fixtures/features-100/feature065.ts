function id65<T>(x: T): T { return x; }
class Base65<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child65 extends Base65<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E65 { A, B = 3, C, Label = "label" }
const x65: number = id65<number>(65);
const c65 = new Child65("n", x65);
console.log(c65.name, E65.A, E65[3]);
