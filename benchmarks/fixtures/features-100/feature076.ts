function id76<T>(x: T): T { return x; }
class Base76<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child76 extends Base76<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E76 { A, B = 3, C, Label = "label" }
const x76: number = id76<number>(76);
const c76 = new Child76("n", x76);
console.log(c76.name, E76.A, E76[3]);
