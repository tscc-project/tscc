function id79<T>(x: T): T { return x; }
class Base79<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child79 extends Base79<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E79 { A, B = 3, C, Label = "label" }
const x79: number = id79<number>(79);
const c79 = new Child79("n", x79);
console.log(c79.name, E79.A, E79[3]);
