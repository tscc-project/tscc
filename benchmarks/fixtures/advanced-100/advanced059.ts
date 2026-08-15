namespace N59 {
  export const base: number = 59;
  export function add(x: number): number { return base + x; }
}
function over59(x: number): number;
function over59(x: string): string;
function over59(x: number | string): number | string { return x; }
abstract class A59 { abstract f(x: number): number; }
class B59 extends A59 { override f(x: number): number { return x + 1; } }
enum E59 { A = 5 / 2, B, C = 2 ** 3, D }
const angle59 = <number>(59 + 1);
const check59 = {x: angle59} satisfies {x: number};
console.log(N59.add(1), over59(59), new B59().f(1), E59.B, E59.D, check59.x);
