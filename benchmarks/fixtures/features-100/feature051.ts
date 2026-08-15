function id51<T>(x: T): T { return x; }
class Base51<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child51 extends Base51<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E51 { A, B = 3, C, Label = "label" }
const x51: number = id51<number>(51);
const c51 = new Child51("n", x51);
console.log(c51.name, E51.A, E51[3]);
