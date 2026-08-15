function id93<T>(x: T): T { return x; }
class Base93<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child93 extends Base93<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E93 { A, B = 3, C, Label = "label" }
const x93: number = id93<number>(93);
const c93 = new Child93("n", x93);
console.log(c93.name, E93.A, E93[3]);
