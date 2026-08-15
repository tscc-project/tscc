interface Item300 { id: number; name: string; }
type Key300 = string | number;
export function make300(id: number, name: string): Item300 {
  const value: Item300 = {id, name};
  return value as Item300;
}
export const item300: Item300 = make300(300, "item-300");
