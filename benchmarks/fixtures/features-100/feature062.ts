function id62<T>(x: T): T { return x; }
class Base62<T> {
  constructor(public value: T, readonly tag: string) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child62 extends Base62<number> {
  constructor(public readonly name: string, value: number) { super(value, "t"); }
}
enum E62 { A, B = 3, C, Label = "label" }
const x62: number = id62<number>(62);
const c62 = new Child62("n", x62);
console.log(c62.name, E62.A, E62[3]);
