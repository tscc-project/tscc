interface Item334 { id: number; name: string; }
type Key334 = string | number;
export function make334(id: number, name: string): Item334 {
  const value: Item334 = {id, name};
  return value as Item334;
}
export const item334: Item334 = make334(334, "item-334");
