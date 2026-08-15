function id96<T>(x: T): T { return x; }
class Base96<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child96 extends Base96<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E96 { A, B = 3, C, Label = "label" }
const x96: number = id96<number>(96);
const c96 = new Child96("n", x96);
console.log(c96.name, E96.A, E96[3]);
