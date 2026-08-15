interface Item217 { id: number; name: string; }
type Key217 = string | number;
export function make217(id: number, name: string): Item217 {
  const value: Item217 = {id, name};
  return value as Item217;
}
export const item217: Item217 = make217(217, "item-217");
