interface Item233 { id: number; name: string; }
type Key233 = string | number;
export function make233(id: number, name: string): Item233 {
  const value: Item233 = {id, name};
  return value as Item233;
}
export const item233: Item233 = make233(233, "item-233");
