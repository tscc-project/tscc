interface Item331 { id: number; name: string; }
type Key331 = string | number;
export function make331(id: number, name: string): Item331 {
  const value: Item331 = {id, name};
  return value as Item331;
}
export const item331: Item331 = make331(331, "item-331");
