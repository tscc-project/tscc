interface Item356 { id: number; name: string; }
type Key356 = string | number;
export function make356(id: number, name: string): Item356 {
  const value: Item356 = {id, name};
  return value as Item356;
}
export const item356: Item356 = make356(356, "item-356");
