interface Item408 { id: number; name: string; }
type Key408 = string | number;
export function make408(id: number, name: string): Item408 {
  const value: Item408 = {id, name};
  return value as Item408;
}
export const item408: Item408 = make408(408, "item-408");
