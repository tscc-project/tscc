interface Item33 { id: number; name: string; }
type Key33 = string | number;
export function make33(id: number, name: string): Item33 {
  const value: Item33 = {id, name};
  return value as Item33;
}
export const item33: Item33 = make33(33, "item-33");
