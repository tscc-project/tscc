namespace N96 {
  export const base: number = 96;
  export function add(x: number): number { return base + x; }
}
function over96(x: number): number;
function over96(x: string): string;
function over96(x: number | string): number | string { return x; }
abstract class A96 { abstract f(x: number): number; }
class B96 extends A96 { override f(x: number): number { return x + 1; } }
enum E96 { A = 5 / 2, B, C = 2 ** 3, D }
const angle96 = <number>(96 + 1);
const check96 = {x: angle96} satisfies {x: number};
console.log(N96.add(1), over96(96), new B96().f(1), E96.B, E96.D, check96.x);
