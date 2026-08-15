function id13<T>(x: T): T { return x; }
class Base13<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child13 extends Base13<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E13 { A, B = 3, C, Label = "label" }
const x13: number = id13<number>(13);
const c13 = new Child13("n", x13);
console.log(c13.name, E13.A, E13[3]);
