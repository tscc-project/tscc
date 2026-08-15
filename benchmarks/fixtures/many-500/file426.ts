interface Item426 { id: number; name: string; }
type Key426 = string | number;
export function make426(id: number, name: string): Item426 {
  const value: Item426 = {id, name};
  return value as Item426;
}
export const item426: Item426 = make426(426, "item-426");
