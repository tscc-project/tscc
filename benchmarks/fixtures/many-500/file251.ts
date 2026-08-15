interface Item251 { id: number; name: string; }
type Key251 = string | number;
export function make251(id: number, name: string): Item251 {
  const value: Item251 = {id, name};
  return value as Item251;
}
export const item251: Item251 = make251(251, "item-251");
