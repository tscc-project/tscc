function id82<T>(x: T): T { return x; }
class Base82<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child82 extends Base82<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E82 { A, B = 3, C, Label = "label" }
const x82: number = id82<number>(82);
const c82 = new Child82("n", x82);
console.log(c82.name, E82.A, E82[3]);
