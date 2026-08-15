interface Item275 { id: number; name: string; }
type Key275 = string | number;
export function make275(id: number, name: string): Item275 {
  const value: Item275 = {id, name};
  return value as Item275;
}
export const item275: Item275 = make275(275, "item-275");
