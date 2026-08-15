interface Item139 { id: number; name: string; }
type Key139 = string | number;
export function make139(id: number, name: string): Item139 {
  const value: Item139 = {id, name};
  return value as Item139;
}
export const item139: Item139 = make139(139, "item-139");
