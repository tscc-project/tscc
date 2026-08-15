function id78<T>(x: T): T { return x; }
class Base78<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child78 extends Base78<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E78 { A, B = 3, C, Label = "label" }
const x78: number = id78<number>(78);
const c78 = new Child78("n", x78);
console.log(c78.name, E78.A, E78[3]);
