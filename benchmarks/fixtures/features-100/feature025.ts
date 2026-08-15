function id25<T>(x: T): T { return x; }
class Base25<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child25 extends Base25<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E25 { A, B = 3, C, Label = "label" }
const x25: number = id25<number>(25);
const c25 = new Child25("n", x25);
console.log(c25.name, E25.A, E25[3]);
