function id71<T>(x: T): T { return x; }
class Base71<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child71 extends Base71<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E71 { A, B = 3, C, Label = "label" }
const x71: number = id71<number>(71);
const c71 = new Child71("n", x71);
console.log(c71.name, E71.A, E71[3]);
