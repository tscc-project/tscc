namespace N67 {
  export const base: number = 67;
  export function add(x: number): number { return base + x; }
}
function over67(x: number): number;
function over67(x: string): string;
function over67(x: number | string): number | string { return x; }
abstract class A67 { abstract f(x: number): number; }
class B67 extends A67 { override f(x: number): number { return x + 1; } }
enum E67 { A = 5 / 2, B, C = 2 ** 3, D }
const angle67 = <number>(67 + 1);
const check67 = {x: angle67} satisfies {x: number};
console.log(N67.add(1), over67(67), new B67().f(1), E67.B, E67.D, check67.x);
