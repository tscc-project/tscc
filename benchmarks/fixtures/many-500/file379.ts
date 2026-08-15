interface Item379 { id: number; name: string; }
type Key379 = string | number;
export function make379(id: number, name: string): Item379 {
  const value: Item379 = {id, name};
  return value as Item379;
}
export const item379: Item379 = make379(379, "item-379");
