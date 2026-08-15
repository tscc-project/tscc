interface Item306 { id: number; name: string; }
type Key306 = string | number;
export function make306(id: number, name: string): Item306 {
  const value: Item306 = {id, name};
  return value as Item306;
}
export const item306: Item306 = make306(306, "item-306");
