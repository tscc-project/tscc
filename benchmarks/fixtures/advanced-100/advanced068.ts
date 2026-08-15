namespace N68 {
  export const base: number = 68;
  export function add(x: number): number { return base + x; }
}
function over68(x: number): number;
function over68(x: string): string;
function over68(x: number | string): number | string { return x; }
abstract class A68 { abstract f(x: number): number; }
class B68 extends A68 { override f(x: number): number { return x + 1; } }
enum E68 { A = 5 / 2, B, C = 2 ** 3, D }
const angle68 = <number>(68 + 1);
const check68 = {x: angle68} satisfies {x: number};
console.log(N68.add(1), over68(68), new B68().f(1), E68.B, E68.D, check68.x);
