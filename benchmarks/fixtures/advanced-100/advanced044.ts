namespace N44 {
  export const base: number = 44;
  export function add(x: number): number { return base + x; }
}
function over44(x: number): number;
function over44(x: string): string;
function over44(x: number | string): number | string { return x; }
abstract class A44 { abstract f(x: number): number; }
class B44 extends A44 { override f(x: number): number { return x + 1; } }
enum E44 { A = 5 / 2, B, C = 2 ** 3, D }
const angle44 = <number>(44 + 1);
const check44 = {x: angle44} satisfies {x: number};
console.log(N44.add(1), over44(44), new B44().f(1), E44.B, E44.D, check44.x);
