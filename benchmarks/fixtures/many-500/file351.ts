interface Item351 { id: number; name: string; }
type Key351 = string | number;
export function make351(id: number, name: string): Item351 {
  const value: Item351 = {id, name};
  return value as Item351;
}
export const item351: Item351 = make351(351, "item-351");
