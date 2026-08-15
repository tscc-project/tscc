namespace N99 {
  export const base: number = 99;
  export function add(x: number): number { return base + x; }
}
function over99(x: number): number;
function over99(x: string): string;
function over99(x: number | string): number | string { return x; }
abstract class A99 { abstract f(x: number): number; }
class B99 extends A99 { override f(x: number): number { return x + 1; } }
enum E99 { A = 5 / 2, B, C = 2 ** 3, D }
const angle99 = <number>(99 + 1);
const check99 = {x: angle99} satisfies {x: number};
console.log(N99.add(1), over99(99), new B99().f(1), E99.B, E99.D, check99.x);
