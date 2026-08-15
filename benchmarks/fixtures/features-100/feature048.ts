function id48<T>(x: T): T { return x; }
class Base48<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child48 extends Base48<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E48 { A, B = 3, C, Label = "label" }
const x48: number = id48<number>(48);
const c48 = new Child48("n", x48);
console.log(c48.name, E48.A, E48[3]);
