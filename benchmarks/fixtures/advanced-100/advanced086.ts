namespace N86 {
  export const base: number = 86;
  export function add(x: number): number { return base + x; }
}
function over86(x: number): number;
function over86(x: string): string;
function over86(x: number | string): number | string { return x; }
abstract class A86 { abstract f(x: number): number; }
class B86 extends A86 { override f(x: number): number { return x + 1; } }
enum E86 { A = 5 / 2, B, C = 2 ** 3, D }
const angle86 = <number>(86 + 1);
const check86 = {x: angle86} satisfies {x: number};
console.log(N86.add(1), over86(86), new B86().f(1), E86.B, E86.D, check86.x);
