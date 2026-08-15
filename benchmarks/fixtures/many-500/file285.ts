interface Item285 { id: number; name: string; }
type Key285 = string | number;
export function make285(id: number, name: string): Item285 {
  const value: Item285 = {id, name};
  return value as Item285;
}
export const item285: Item285 = make285(285, "item-285");
